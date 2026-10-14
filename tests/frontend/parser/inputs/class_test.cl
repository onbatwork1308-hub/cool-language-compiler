class Main {
	x : Int <- 10;
	get_x() : Int {
		x
	};

	main() : Object {
		(new IO).out_int(get_x())
	};
};
 
