#/bin/sh

if [ $# -eq 1 ]; then
	cd $1
else
	exit 1
fi

find . -maxdepth 1 -iname '*.tex' -print0 | xargs -0 --verbose -r -L1 -P8 pdflatex -interaction=nonstopmode 1> /dev/null
rm *.aux *.log 2> /dev/null
 
for dotFile in *.dot
do
	[ -e "$dotFile" ] || break
	echo "$dotFile"
	dot $dotFile -Tpdf -o "${dotFile%.*}.pdf"
done

mkdir -p GraphDot/
mkdir -p GraphPdf/
mkdir -p ResourceUtilizationTex/
mkdir -p ResourceUtilizationPdf/
mkdir -p ResourceUtilizationILPTex/
mkdir -p ResourceUtilizationILPPdf/
mkdir -p ScheduleAnalysisTex/
mkdir -p ScheduleAnalysisPdf/
mkdir -p GroupAnalysisTex/
mkdir -p GroupAnalysisPdf/
mkdir -p CompleteAnalysisTex/
mkdir -p CompleteAnalysisPdf/

mv *_GH.dot GraphDot/ 2> /dev/null
mv *_GH.pdf GraphPdf/ 2> /dev/null
mv *_RU.tex ResourceUtilizationTex/ 2> /dev/null
mv *_RU.pdf ResourceUtilizationPdf/ 2> /dev/null
mv *_RU_ILP.tex ResourceUtilizationILPTex/ 2> /dev/null
mv *_RU_ILP.pdf ResourceUtilizationILPPdf/ 2> /dev/null
mv *_ST.tex ScheduleAnalysisTex/ 2> /dev/null
mv *_ST.pdf ScheduleAnalysisPdf/ 2> /dev/null
mv stat_*.tex GroupAnalysisTex/ 2> /dev/null
mv stat_*.pdf GroupAnalysisPdf/ 2> /dev/null
mv complete_stat.tex CompleteAnalysisTex/ 2> /dev/null
mv complete_stat.pdf CompleteAnalysisPdf/ 2> /dev/null

