from shutil import copyfile
import os

Import("env")
Import("projenv")
print("||||||| IoToTa: entering post script  ||||||||")
#print("-- Environement env Dump ---")
#print(env.Dump())
#print("-- Environement projenv Dump ---")
#print(projenv.Dump())

#sou = ".pio\\build\\lolin_d32\\firmware.bin"
#dst = "Data\\fw.bin"

def after_build(source, target, env): 
    """After Build, Autaumaticaly coppy the bin file to the daa dir then update the version file"""
    print("###### Finished building!! Copy the bin file in the Data directory ########")
    #print("Source  : {}".format(source               ))
    #print("Target  : {}".format(target               ))
    #print("Env     : {}".format(env                     ))
    #print("buildgPath: {}".format(env["PROJECT_BUILD_DIR"] ))
    #print("env path: {}".format(env["PIOENV"] ))
    #print("Env Dump: {}".format(env.Dump()           ))
    #path = os.getcwd()
    #print("Current path:{0}".format(path)) 
    #res = Copy(target, dst)
    #res = copyfile(sou, dst)  # copy the bin file TODO adapt to target
    fileName   = env["PROGNAME"] + env["PROGSUFFIX"]
    sourcePath = os.path.join(env["PROJECT_BUILD_DIR"], env["PIOENV"], fileName)
    targetPath = os.path.join(env["PROJECT_DATA_DIR" ], fileName)

    print("Source path: {}".format(sourcePath))
    print("Target path: {}".format(targetPath))

    res = copyfile(sourcePath , targetPath)  # copy the bin file TODO adapt to target
    print(res)
    # TODO update the version file
    
def AB(source, target, env): 
    """After Build, Autaumaticaly coppy the bin file to the daa dir then update the version file"""
    print("+++++++++++++++++++ Finished building!! Copy the bin file in the Data directory ++++++++++++++++ ")
    fileName   = env["PROGNAME"] + env["PROGSUFFIX"]
    sourcePath = os.path.join(env["PROJECT_BUILD_DIR"], env["PIOENV"], fileName)
    targetPath = os.path.join(env["PROJECT_DATA_DIR" ], fileName)

    print("Source path: {}".format(sourcePath))
    print("Target path: {}".format(targetPath))

    res = copyfile(sourcePath , targetPath) 
    print(res)
    
      

#env.AddPostAction("buildprog", after_build)
#env.AddPostAction("build", after_build)
#env.AddPostAction("${BUILD_DIR}/${PROGNAME}${PROGSUFFIX}", after_build)
#env.AddPostAction("$BUILD_DIR/${PROGNAME}.elf",
#	env.VerboseAction("copy $BUILD_DIR/${PROGNAME}.elf $PROJECT_DATA_DIR/${PROGNAME}.elf", 
#	"Moving target to: $PROJECT_DATA_DIR/${PROGNAME}.elf")
#)
#env.AddPostAction("$BUILD_DIR/${PROGNAME}.elf", AB )
