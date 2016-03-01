echo "Initializing test..."

case $1 in
	"ct")
		echo "Performing Create Test"
		./tests
		python graph_test.py

		;;
	"dtt")
		echo "Performing Drop Table Test"
		python graph_test.py
		
		;;
	*)
		echo "No test selected options are:"
		echo "ct: Create Test"
		echo "dtt: Drop Table Test"

esac

