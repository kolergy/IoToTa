from shutil import copyfile
import os

Import("env")
sou = ".pio\\build\\lolin_d32\\firmware.bin"
dst = "Data\\fw.bin"
def after_build(source, target, env): 
    """After Build, Autaumaticaly coppy the bin file to the daa dir then update the version file"""
    print("Finished building!! Copy the bin file in the Data directory")
    #print("Source:{0}".format(source))
    #print("Target:{0}".format(target))
    #print("Env:   {0}".format(env   ))
    #path = os.getcwd()
    #print("Current path:{0}".format(path)) 
    #res = Copy(target, dst)
    res = copyfile(sou, dst)  # copy the bin file TODO adapt to target
    print(res)
    # TODO update the version file
    

env.AddPostAction("buildprog", after_build)