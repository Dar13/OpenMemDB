echo "Initializing test..."

case $1 in
	"ct")
		echo "Performing Create Test"
		./tests pct 1
		./tests pct 2
		./tests pct 4
		# ./tests pct 8
		python graph_test.py

		;;
	"dtt")
		echo "Performing Drop Table Test"
		./tests pdt 1
		./tests pdt 2
		./tests pdt 4
		python graph_test.py
		
		;;
	"irt")
		echo "Performing Insert Row Test"
		time ./tests pirt 1
		time ./tests pirt 2
		time ./tests pirt 4
		python graph_test.py
		;;

	*)
		echo "No test selected options are:"
		echo "ct: Create Test"
		echo "dtt: Drop Table Test"
		echo "irt: Insert Row Test"

esac

