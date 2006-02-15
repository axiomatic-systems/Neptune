#! /bin/sh
TARGET=$1
if [ -z $TARGET ]; then
echo "target not specified"
exit 1
fi
 
BUILD_ROOT=".."
DIRS="Neptune Core System Apps/NetPump Apps/NetConfig Tests/Messages1 Tests/Strings1 Tests/Threads1 Tests/Xml1 Tests/Arrays1 Tests/Misc1 Tests/RingBuffer1 Tests/HttpClient1"
 
for dir in $DIRS
do
echo $dir --------------------------------------
out=$BUILD_ROOT/$dir/Targets/$TARGET
template=$BUILD_ROOT/$dir/Targets/Template
if ! [ -d $template ]; then
  echo "Template dir $template does not exist"
fi
[ -d $BUILD_ROOT/Includes/Targets/$TARGET ] || mkdir $BUILD_ROOT/Includes/Targets/$TARGET
[ -d $BUILD_ROOT/Includes/Targets/$TARGET/BuildConfigs ] || mkdir $BUILD_ROOT/Includes/Targets/$TARGET/BuildConfigs
[ -d $out ] || mkdir $out
[ ! -f $out/Makefile ] || [ $template/Makefile -nt $out/Makefile ] && (cat $template/Makefile | sed "s/@TARGET@/$TARGET/" >$out/Makefile; echo ++++++++ Makefile updated)
done
 
 
 
 
 
 
 
