from shutil import copyfile
import os

Import("env")

def after_build(source, target, env): 
    print("[From Script] Finished building!!")
    print("Source:{0}".format(source))
    print("Target:{0}".format(target))
    print("Env:   {0}".format(env   ))
    path = os.getcwd()
    print("Current path:{0}".format(path)) 

    #copyfile(src, dst)

env.AddPostAction("buildprog", after_build)