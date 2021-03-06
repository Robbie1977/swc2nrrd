#include <stdio.h>
#include <stdint.h>
#include <math.h>

//gcc -shared -fPIC -I/usr/include/python2.7/ -lpython2.7 -o cVersion3.so cVersion3.c

/*
 * based on (python):
def getUniquesLowMemory(inputData,counts):
    output = np.zeros(counts,dtype=np.int32)
    raveledInput = inputData.ravel()
    for i in range(len(raveledInput)):
        output[raveledInput[i]]+=1
    return counts
 *
 */
int uniqueVals(const int32_t * inputArrP, const long arrayLength, int32_t * outDataP) {
    int32_t * inputArr = (int32_t *) inputArrP;
    int32_t * outData = (int32_t *) outDataP;
	int i;

	for (i=0;i<arrayLength;i++){
		outData[inputArr[i]] +=1;
	}
	return 0;


}

/*
 * based on (python):
 * https://en.wikipedia.org/wiki/Euler%E2%80%93Rodrigues_formula
 * 
 def getRotMat(vectIn,angIn):
    a = np.cos(angIn)
    b = vectIn[0]*np.sin(angIn)
    c = vectIn[1]*np.sin(angIn)
    d = vectIn[2]*np.sin(angIn)
    return np.matrix([[a*a+b*b-c*c-d*d,2*(b*c-d*a),2*(b*d+a*c)],\
                      [2*(b*c+a*d),a*a+c*c-b*b-d*d,2*(c*d-a*b)],\
                      [2*(b*d-a*c),2*(c*d+a*b),a*a+d*d-c*c-b*b]])
 * 
 */
int getRotMat(double * vectInP,double angIn,double * vectOutP){
	double * vectIn = (double *) vectInP;
	double * vectOut = (double *) vectOutP;
	double a = (double)cos(angIn);
	double b = vectIn[0]*sin(angIn);
    double c = vectIn[1]*sin(angIn);
    double d = vectIn[2]*sin(angIn);
    vectOut[0] = a*a+b*b-c*c-d*d;
    vectOut[1] = 2*(b*c-d*a);
    vectOut[2] = 2*(b*d+a*c);
    vectOut[3] = 2*(b*c+a*d);
    vectOut[4] = a*a+c*c-b*b-d*d;
    vectOut[5] = 2*(c*d-a*b);
    vectOut[6] = 2*(b*d-a*c);
    vectOut[7] = 2*(c*d+a*b);
    vectOut[8] = a*a+d*d-c*c-b*b;
    return 0;
}

int doRot(double * rotMatP,double * vectInP){
	double * rotMat = (double *) rotMatP;
	double * vectIn = (double *) vectInP;
	double vectOut[3];
    vectOut[0] = rotMat[0]*vectIn[0]+rotMat[1]*vectIn[1]+rotMat[2]*vectIn[2];
    vectOut[1] = rotMat[3]*vectIn[0]+rotMat[4]*vectIn[1]+rotMat[5]*vectIn[2];
    vectOut[2] = rotMat[6]*vectIn[0]+rotMat[7]*vectIn[1]+rotMat[8]*vectIn[2];
	vectInP[0] = vectOut[0];
	vectInP[1] = vectOut[1];
	vectInP[2] = vectOut[2];
    return 0;
}

/*
 * based on (python):
*         rotMat = getRotMat(derDir/np.sqrt(np.sum(np.power(derDir,2))),0.1)
m2 = np.matrix.transpose(np.matrix(perpIN))
 * for theta in np.arange(0,np.pi/2.0,0.01):
            for intM in np.arange(-width,width,0.5):
                m3 = m2*intM
                newCo = tuple(np.round(np.array([centrePoint[0]+m3[0,0],centrePoint[1]+m3[1,0],centrePoint[2]+m3[2,0]])))
                outputImg[newCo] = 1
            m2 = np.dot(rotMat,m2)
*
* 
* perpDir - perpendicular direction
* dirCirc - initial direction
* outputImgP - pointer to output image (uint8)
* outputSize - outputDims
* rotAmount - the amount of rotation to do each time in radians
* radius - radius of the pipe
* incAmount - increment on position to do each time
* */
double doCirc(double perpDir[3],double dirCirc[3],double centrePoint[3], uint8_t * outputImgP,int outputSize[3],
				double rotAmount,double radius,double incAmount,double imgDimScales[3]){
	uint8_t * outputImg = (uint8_t *) outputImgP;
	double rotMatT[9];
	double * rotMat = rotMatT;
	getRotMat(perpDir,rotAmount,rotMat);
	double curAng = 0.0;
	double curPos[3];
	double incPos[3];
	double curI;
	do {
		// Update values for this angle:
		curPos[0] = -radius*dirCirc[0]*imgDimScales[0]+centrePoint[0];
		curPos[1] = -radius*dirCirc[1]*imgDimScales[1]+centrePoint[1];
		curPos[2] = -radius*dirCirc[2]*imgDimScales[2]+centrePoint[2];
		incPos[0] = incAmount*dirCirc[0]*imgDimScales[0];
		incPos[1] = incAmount*dirCirc[1]*imgDimScales[1];
		incPos[2] = incAmount*dirCirc[2]*imgDimScales[2];
		curI = -radius;
		do {
			//Set the value:
			int x = (int) roundf(curPos[0]);
			int y = (int) roundf(curPos[1]);
			int z = (int) roundf(curPos[2]);
			int indexx = z+(y+x*outputSize[1])*outputSize[2];
				/*
				if (indexx>=outputSize[0]*outputSize[1]*outputSize[2]||indexx<0){
					vectOut[0]=curPos[0];
					vectOut[1]=curPos[1];
					vectOut[2]=curPos[2];
					vectOut[3]=dirCirc[0];
					vectOut[4]=dirCirc[1];
					vectOut[5]=dirCirc[2];
				}
				*/
				if (indexx<outputSize[0]*outputSize[1]*outputSize[2]&&indexx>0){
					outputImg[indexx] = 1;
				}			
			//Increment:
			curPos[0]+=incPos[0];
			curPos[1]+=incPos[1];
			curPos[2]+=incPos[2];
			curI+=incAmount;
		}
		while(curI<radius);
		curAng+=rotAmount;
		doRot(rotMat,dirCirc);
	}
	while(curAng<M_PI);
	return 1.0;
}

double addSphere(double centrePoint[3], uint8_t * outputImgP,int outputSize[3],double rotAmount,
				   double radius,double incAmount,double imgDimScales[3]){
	uint8_t * outputImg = (uint8_t *) outputImgP;
	// axis about which the perp. direction rotates.
	double initRDirT[3] = {0.0,1.0,0.0};
	// direction perpendicular to the circle being drawn - rotates about the below:
	double rotDirTX[3] = {1.0,0.0,0.0};
	// Pointers:
	double * initRDir = initRDirT;
	double * rotDirX = rotDirTX;
	// for rotating the perp. direction:
	double rotMatAT[9];
	double * rotMatA = rotMatAT;
	getRotMat(initRDir,rotAmount,rotMatA);
	double curAngA = 0.0;
	double curPos[3];
	double incPos[3];
	double curI;
	do {
		double rotMatT[9];
		double * rotMat = rotMatT;
		getRotMat(rotDirX,rotAmount,rotMat);
		// initial circle direction - always the same
		double dirCircT[3] = {0.0,1.0,0.0};
		double * dirCirc = dirCircT;
		double curAng = 0.0;
		do {
			// Update values for this angle:
			curPos[0] = -radius*dirCirc[0]*imgDimScales[0]+centrePoint[0];
			curPos[1] = -radius*dirCirc[1]*imgDimScales[1]+centrePoint[1];
			curPos[2] = -radius*dirCirc[2]*imgDimScales[2]+centrePoint[2];
			incPos[0] = incAmount*dirCirc[0]*imgDimScales[0];
			incPos[1] = incAmount*dirCirc[1]*imgDimScales[1];
			incPos[2] = incAmount*dirCirc[2]*imgDimScales[2];
			curI = -radius;
			do {
				//Set the value:
				int x = (int) roundf(curPos[0]);
				int y = (int) roundf(curPos[1]);
				int z = (int) roundf(curPos[2]);
				int indexx = z+(y+x*outputSize[1])*outputSize[2];
				/*
				if (indexx>=outputSize[0]*outputSize[1]*outputSize[2]||indexx<0){
					vectOut[0]=curPos[0];
					vectOut[1]=curPos[1];
					vectOut[2]=curPos[2];
					vectOut[3]=dirCirc[0];
					vectOut[4]=dirCirc[1];
					vectOut[5]=dirCirc[2];
				}
				*/
				if (indexx<outputSize[0]*outputSize[1]*outputSize[2]&&indexx>0){
					outputImg[indexx] = 1;
				}
				//Increment:
				curPos[0]+=incPos[0];
				curPos[1]+=incPos[1];
				curPos[2]+=incPos[2];
				curI+=incAmount;
			}
			while(curI<radius);
			curAng+=rotAmount;
			doRot(rotMat,dirCirc);
		}
		while(curAng<M_PI);
		curAngA+=rotAmount;
		doRot(rotMatA,rotDirX);
	}
	while(curAngA<M_PI);
	return 1.0;
}

