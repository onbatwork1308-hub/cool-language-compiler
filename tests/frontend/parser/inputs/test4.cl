class Main {
    classify(x : Int) : String {
        let result : String <- "unknown" in {
            case x of
                n : Int => if n = 0 then "zero" else "nonzero" fi;
            esac;
            result;
        }
    };
};