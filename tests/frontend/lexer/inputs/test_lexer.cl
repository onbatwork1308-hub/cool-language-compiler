class Main inherits IO {

    -- Basic identifiers and integers
    x : Int <- 10;
    y : Int <- 20;
    result : Int <- x + y * 2 / 5 - 3;

    -- Type identifiers vs object identifiers
    main : String;
    anotherVariable : Bool <- true;
    SomeTypeName : Int;

    -- Arithmetic and comparison operators
    a : Int <- 10;
    b : Int <- 20;

    test1 : Bool <- a < b;
    test2 : Bool <- a <= b;
    test3 : Bool <- a = b;

    -- Assignment
    x <- 100;

    -- Unary operators
    flag : Bool <- not false;
    value : Int <- ~10;

    -- Dispatch operators
    out_string("Hello");
    self.out_int(x);
    self@IO.out_string("Static dispatch");

    -- Strings
    msg : String <- "Hello, COOL!";
    escaped : String <- "Line1\nLine2\tTabbed";
    quote : String <- "He said: \"Hello\"";
    slash : String <- "This is a backslash: \\";
    arbitrary : String <- "\x";

    -- Nested block comments
    (*
        Outer comment

        (*
            Inner comment

            (*
                Even deeper
            *)
        *)

        Back to outer comment
    *)

    -- Case expression
    case x of
        n : Int => n + 1;
        s : String => 0;
    esac;

    -- Let expression
    let a : Int <- 10,
        b : Int <- 20
    in {
        a <- a + b;
        a;
    };

    -- Conditional
    if x < y then
        x <- x + 1
    else
        x <- x - 1
    fi;

    -- Loop
    while x < 100 loop
        x <- x + 1
    pool;

    -- More punctuation
    {
        (
            x + y
        );
    };

};
