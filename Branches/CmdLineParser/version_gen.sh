svn info | grep Date | sed "s/^.*Date: /#define SVN_DATE     (wxT(\"/g" | sed "s/ [0-9].*/\"))/g" > version.autogen

svn info | grep Revision | sed "s/^.* /#define SVN_REVISION (/g" | sed "s/$/)/g" >> version.autogen

echo "#define SVN_VERSION  (wxT(\"Svn Version\"))" >> version.autogen
echo "#define SVN_MANAGED  (true)" >> version.autogen
