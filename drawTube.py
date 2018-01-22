'''
Created on 20 Jan 2018

@author: fewtrem
'''

import numpy as np

def interpBez(bezPoints,t):
    tm = 1-t
    return tm*tm*tm*bezPoints[0]+3*tm*tm*t*bezPoints[1]+3*t*t*tm*bezPoints[2]+t*t*t*bezPoints[3]
def interpBezDer(bezPoints,t):
    tm = 1-t
    return 3*tm*tm*(bezPoints[1]-bezPoints[0])+6*tm*t*(bezPoints[2]-bezPoints[1])+3*t*t*(bezPoints[3]-bezPoints[2])

import ctypes
lib = ctypes.cdll.LoadLibrary('cVersion3.so')
cdoCirc = lib.doCirc
caddSphere = lib.addSphere
def doCrossCircC(t,outputImg,radius,bezCurvePoints):
    centrePoint = interpBez(bezCurvePoints,t)
    perpDir = interpBezDer(bezCurvePoints,t)
    sumD = np.sum(np.power(perpDir,2))
    # check it has a length:
    if sumD !=0:
        rotAmount = 0.01
        incAmount = 0.5
        # Calculatable things:
        outputSize = outputImg.shape
        tDir = perpDir.copy()
        tMax = np.argmax(np.abs(perpDir))
        tDir[tMax] = -perpDir[tMax]
        initDir = np.cross(tDir,perpDir)
        divInitDirN = np.sqrt(np.sum(np.power(initDir,2)))
        if divInitDirN!=0:
            initDirN = initDir/divInitDirN
        else:
            initDirN = np.copy(initDir)
        divPerpDir = np.sqrt(np.sum(np.power(perpDir,2)))
        if divPerpDir!=0:
            perpDirN = perpDir/divPerpDir
        else:
            perpDirN = np.copy(perpDir)
        dimsScale = np.array([1.0,1.0,1.0/1.707],dtype=np.float)
        # Pointers:
        perpDirNP = (ctypes.c_double * len(perpDirN))(*perpDirN)
        initDirNP = (ctypes.c_double * len(initDirN))(*initDirN)
        centrePointP = (ctypes.c_double * len(centrePoint))(*centrePoint)
        outputSizeP = (ctypes.c_int * len(outputSize))(*outputSize)
        dimsScaleP = (ctypes.c_double * len(dimsScale))(*dimsScale)
        # Do the function:
        cdoCirc(perpDirNP,
                initDirNP,
                centrePointP,
                ctypes.c_void_p(outputImg.ctypes.data),
                outputSizeP,
                ctypes.c_double(rotAmount),
                ctypes.c_double(radius),
                ctypes.c_double(incAmount),
                dimsScaleP)

def doAddSphereC(outputImg,radius,centrePoint):
    rotAmount = 0.01
    incAmount = 0.5
    # Calculatable things:
    outputSize = outputImg.shape
    dimsScale = np.array([1.0,1.0,1.0/1.707],dtype=np.float)
    # Pointers:
    centrePointP = (ctypes.c_double * len(centrePoint))(*centrePoint)
    outputSizeP = (ctypes.c_int * len(outputSize))(*outputSize)
    dimsScaleP = (ctypes.c_double * len(dimsScale))(*dimsScale)
    # Do the function:
    '''
    addSphere(double centrePoint[3], uint8_t * outputImgP,int outputSize[3],double rotAmount,
                   double radius,double incAmount,double imgDimScales[3])
    '''
    caddSphere(centrePointP,
        ctypes.c_void_p(outputImg.ctypes.data),
        outputSizeP,
        ctypes.c_double(rotAmount),
        ctypes.c_double(radius),
        ctypes.c_double(incAmount),
        dimsScaleP)
