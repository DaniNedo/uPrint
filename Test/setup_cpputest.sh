git clone https://github.com/cpputest/cpputest.git

cd cpputest

match='AC_LIBTOOL_DLOPEN'
insert='m4_pattern_allow([AC_LIBTOOL_DLOPEN])\nm4_pattern_allow([AC_PROG_LIBTOOL])\nAC_PROG_RANLIB'
file='configure.ac'

sed -i "s/$match/$insert\n$match/" $file

sudo apt-get install autoconf

./autogen.sh
./configure
make
cd ..

echo -e "\n\nCpputest build finished\n\n"
