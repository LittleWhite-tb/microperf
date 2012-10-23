#!/bin/sh

microcreator_output=output/

#The different regression tests
MOVAPD_st=regression/MOVAPD/stencil/
MOVAPD_ur=regression/MOVAPD/unroll/
MOVAPS_st=regression/MOVAPS/stencil/
MOVAPS_ur=regression/MOVAPS/unroll/
MOVSD_st=regression/MOVSD/stencil/
MOVSD_ur=regression/MOVSD/unroll/
MOVSS_st=regression/MOVSS/stencil/
MOVSS_ur=regression/MOVSS/unroll/
shades_grey=regression/shades_grey/
gauss4=regression/gauss4/
openmp=regression/OpenMP/
redefinition=regression/redefinition/

echo "-----------------------------------------------"
echo "--------------- REGRESSION TEST ---------------"
echo "-----------------------------------------------"
echo ""

echo "-> Compiling the microcreator tool"
cd ..
make clean
make 

#------------------ MOVAPD_st ------------------
./microcreator $MOVAPD_st"description_MOVAPD_st_L_LL_LLL.xml" 2>tmp
file1=$microcreator_output"example00000.s"
file2=$MOVAPD_st"example00000.s"

if diff $file1 $file2 >/dev/null ; then
	echo "-> MOVAPD_st: ---------------------- PASSED"
else
	echo "-> MOVAPD_st: ---------------------- FAILED"
fi

#------------------ MOVAPD_ur ------------------
./microcreator $MOVAPD_ur"description_MOVAPD_ur_L_LL_LLL.xml" 2>tmp

file1=$microcreator_output"example00000.s"
file2=$MOVAPD_ur"example00000.s"

if diff $file1 $file2 >/dev/null ; then
	echo "-> MOVAPD_ur: ---------------------- PASSED"
else
	echo "-> MOVAPD_ur: ---------------------- FAILED"
fi

#----------------- MOVAPS_st ------------------
./microcreator $MOVAPS_st"description_MOVAPS_st_L_LL_LLL.xml" 2>tmp

file1=$microcreator_output"example00000.s"
file2=$MOVAPS_st"example00000.s"

if diff $file1 $file2 >/dev/null ; then
	echo "-> MOVAPS_st: ---------------------- PASSED"
else
	echo "-> MOVAPS_st: ---------------------- FAILED"
fi

#------------------ MOVAPS_ur ------------------
./microcreator $MOVAPS_ur"description_MOVAPS_ur_L_LL_LLL.xml" 2>tmp

file1=$microcreator_output"example00000.s"
file2=$MOVAPS_ur"example00000.s"

if diff $file1 $file2 >/dev/null ; then
	echo "-> MOVAPS_ur: ---------------------- PASSED"
else
	echo "-> MOVAPS_ur: ---------------------- FAILED"
fi

#------------------ MOVSD_st -------------------
./microcreator $MOVSD_st"description_MOVSD_st_L_LL_LLL.xml" 2>tmp

file1=$microcreator_output"example00000.s"
file2=$MOVSD_st"example00000.s"

if diff $file1 $file2 >/dev/null ; then
	echo "-> MOVSD_st: ----------------------- PASSED"
else
	echo "-> MOVSD_st: ----------------------- FAILED"
fi

#------------------ MOVSD_ur -------------------
./microcreator $MOVSD_ur"description_MOVSD_ur_L_LL_LLL.xml" 2>tmp

file1=$microcreator_output"example00000.s"
file2=$MOVSD_ur"example00000.s"

if diff $file1 $file2 >/dev/null ; then
	echo "-> MOVSD_ur: ----------------------- PASSED"
else
	echo "-> MOVSD_ur: ----------------------- FAILED"
fi

#------------------ MOVSS_st -------------------
./microcreator $MOVSS_st"description_MOVSS_st_L_LL_LLL.xml" 2>tmp

file1=$microcreator_output"example00000.s"
file2=$MOVSS_st"example00000.s"

if diff $file1 $file2 >/dev/null ; then
	echo "-> MOVSS_st: ----------------------- PASSED"
else
	echo "-> MOVSS_st: ----------------------- FAILED"
fi

#------------------ MOVSS_ur -------------------
./microcreator $MOVSS_ur"description_MOVSS_ur_L_LL_LLL.xml" 2>tmp

file1=$microcreator_output"example00000.s"
file2=$MOVSS_ur"example00000.s"

if diff $file1 $file2 >/dev/null ; then
	echo "-> MOVSS_ur: ----------------------- PASSED"
else
	echo "-> MOVSS_ur: ----------------------- FAILED"
fi

#----------------- shades_grey -----------------
./microcreator $shades_grey"description_shades_grey.xml" 2>tmp

file1=$microcreator_output"example00000.s"
file2=$shades_grey"example00000.s"

if diff $file1 $file2 >/dev/null ; then
	echo "-> shades_grey: -------------------- PASSED"
else
	echo "-> shades_grey: -------------------- FAILED"
fi

#-------------------- gauss4 -------------------
./microcreator $gauss4"micro_creator2.xml" 2>tmp

file1=$microcreator_output"example00000.s"
file2=$gauss4"example00000.s"

if diff $file1 $file2 >/dev/null ; then
	echo "-> gauss4: ------------------------- PASSED"
else
	echo "-> gauss4: ------------------------- FAILED"
fi

#---------------- redefinition -----------------
./microcreator $redefinition"micro_creator2.xml" 2>tmp

file1=$microcreator_output"example00000.s"
file2=$redefinition"example00000.s"

if diff $file1 $file2 >/dev/null ; then
	echo "-> redefinition: ------------------- PASSED"
else
	echo "-> redefinition: ------------------- FAILED"
fi

#-------------------- OpenMP add -------------------
./microcreator $openmp"add_omp.xml" 2>tmp

file1=$microcreator_output"example00000.c"
file2=$openmp"example00000.c"

if diff $file1 $file2 >/dev/null ; then
	echo "-> openmp: ------------------------- PASSED"
else
	echo "-> openmp: ------------------------- FAILED"
fi

#Delete the temporary file 'tmp'
rm tmp
