echo "Initializing test..."

case $1 in
	"ct")
		echo "Performing Create Test"
		./tests pct 1
		./tests pct 2
		./tests pct 4
		./tests pct 8
		./tests pct 16
		./tests pct 32
		./tests pct 64
		python graph_test.py

		;;
	"dtt")
		echo "Performing Drop Table Test"
		./tests pdt 1
		./tests pdt 2
		./tests pdt 4
		./tests pdt 8
		./tests pdt 16
		./tests pdt 32
		./tests pdt 64
		python graph_test.py
		
		;;
	"irt")
		echo "Performing Insert Row Test"
		./tests pirt 1
		./tests pirt 2
	 	./tests pirt 4
	 	./tests pirt 8
	 	./tests pirt 16
	 	./tests pirt 32
	 	./tests pirt 64
		python graph_test.py
		;;

	"st")
		echo "Performing Select Test"
		./tests pst 1
		./tests pst 2
	 	./tests pst 4
	 	./tests pst 8
	 	./tests pst 16
	 	./tests pst 32
	 	./tests pst 64

		python graph_test.py
	 	;;

	"st")
		echo "Performing Mixed Test"
		./tests pmt 1
		./tests pmt 2
	 	./tests pmt 4
	 	./tests pmt 8
	 	./tests pmt 16
	 	./tests pmt 32
	 	./tests pmt 64

		python graph_test.py
	 	;; 	

	*)
		echo "No test selected options are:"
		echo "ct: Create Test"
		echo "dtt: Drop Table Test"
		echo "irt: Insert Row Test"
		echo "st: Select Test"


esac

