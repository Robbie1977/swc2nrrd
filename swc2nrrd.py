from numpy import unique, bincount, shape, min, max, sum, array, uint32, where, uint8, round
import nrrd, sys, os
import numpy as np

def convertswc2nrrd(input,output,template=""):
    with open(input) as fI:
        swcIn = fI.readlines()
    lineDict = {}
    for thisLine in swcIn:
        if thisLine[0]!='#':
            splitLine = thisLine.split(" ")
            lineDict[int(splitLine[0])] = {'position':np.array([splitLine[2],splitLine[3],splitLine[4]],dtype=np.float),
                                  'radius':splitLine[5],
                                  'parent':int(splitLine[6])}
    maxX = 1000
    maxY = 1000
    maxZ = 1000
    if ('.nrrd' in template):
        data1, header1 = nrrd.read(str(template))
        maxX, maxY, maxZ = size(data1)
    else:
        if np.max([x['position'][0] for x in lineDict.values()]) > maxX:
            maxX = np.int64(np.max([x['position'][0] for x in lineDict.values()]))
        if np.max([y['position'][1] for y in lineDict.values()]) > maxY:
            maxY = np.int64(np.max([y['position'][1] for y in lineDict.values()]))
        if np.max([z['position'][2] for z in lineDict.values()]) > maxZ:
            maxZ = np.int64(np.max([z['position'][2] for z in lineDict.values()]))
    print maxX
    print maxY
    print maxZ    
    outputImg = np.zeros((maxX,maxY,maxZ),dtype=np.uint8)
    from drawTube import doCrossCircC,doAddSphereC
    for thisDict in lineDict.values():
        if thisDict['parent'] != -1:
            pA = thisDict['position']
            pB = lineDict[thisDict['parent']]['position']
            for t in np.arange(0.0,1.0,0.3):
                doCrossCircC(t,outputImg,3,[pA,pA,pB,pB])
    nrrd.write(str(output), np.uint8(outputImg), options=header1)

if (len(sys.argv) < 2):
    print 'e.g. python swc2nrrd.py input.swc [output.nrrd] [template.nrrd]'
else:
    fileName, fileExtension = os.path.splitext(sys.argv[1])
    if fileExtension=='.swc':
        convertswc2nrrd(sys.argv[1],str(sys.argv[1]).replace('.swc','.nrrd'))
    elif fileExtension=='.nrrd':
        convertswc2nrrd(str(sys.argv[1]).replace('.nrrd','.swc'),sys.argv[1])  
