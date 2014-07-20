import math
import os
import sys

from crpropa import *
import saga

A=1
Z=1
N=1100

convLength=8.57251e+24 # m
convDensity=2.49651e-27 # kg/m3
convMagneticField=1.11868099322e-09 # Tesla


file='DB.sql'
magF = saga.MagneticField(saga.AMRgrid(file,18))
bField = AMRMagneticField(magF, convLength, convDensity, convMagneticField)

# source and observer parameters
radius=1.*Mpc
origin=Vector3d(50.,50.,50.)*Mpc
direction=Vector3d(-1,0,0)
position=Vector3d(60,50,50)*Mpc
source = Source()
source.addProperty(SourcePosition(position))
source.addProperty(SourceDirection(direction))

# module setup
m = ModuleList()
m.add(DeflectionCK(bField,1e-2,100*kpc,1*Mpc))
m.add(SmallObserverSphere(origin,radius,'Detected','',True))
m.add(ConditionalOutput('TEST-output.txt','Detected'))
m.add(PeriodicBox(origin,Vector3d(200*Mpc)))

# spectrum and composition
minRigidity=100.*EeV
maxRigidity=1000*EeV
spectralIndex=-1.
composition=SourceComposition(minRigidity,maxRigidity,spectralIndex)
composition.add(nucleusId(A,Z),1.)
source.addProperty(composition)

# run
m.showModules()
m.setShowProgress(True)
recursive=True
m.run(source,N,recursive)


