from shutil import copyfile
import os

Import("env")
#sou = "C:\\Users\\jmrog\\Documents\\Github\\IoToTa\\.pio\\build\\lolin_d32\\firmware.bin"
sou = ".pio\\build\\lolin_d32\\firmware.bin"
#dst = "C:\\Users\\jmrog\\Documents\Github\\IoToTa\\Data\\fw.bin"
dst = "Data\\fw.bin"
def after_build(source, target, env): 
    print("[From Script] Finished building!!")
    print("Source:{0}".format(source))
    print("Target:{0}".format(target))
    print("Env:   {0}".format(env   ))
    path = os.getcwd()
    print("Current path:{0}".format(path)) 
    #res = Copy(target, dst)
    #res = Copy(sou, dst)
    res = copyfile(sou, dst)
    print(res)
    

env.AddPostAction("buildprog", after_build)