# Module Dependency Description
# sample:
#   -lcurl
#       depends:libcurl4-openssl-dev
# sample description
#   -lcurl=>the link name. You must write it in "./include.dev",and it will be automatically add When compiled.
#       depends:libcurl4-openssl-dev=>it means libcurl4-openssl-dev must be installed

-ljsoncpp
    depends:libjsoncpp-dev
-lpthread
    lpthread:default include

# This file is for illustration only
# The file "./include.dev" is really useful
# And you have to write dependencies in file "./include.dev", like follow:
#   -ljsoncpp
#   -lpthread