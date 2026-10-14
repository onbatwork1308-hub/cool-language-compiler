class Main {

    -- Calculate base^exp
    power(base : Int, exp : Int) : Int {
        if exp = 0 then
            1
        else
            base * power(base, exp - 1)
        fi
    };

    -- Calculate the number of digits
    count_digits(n : Int) : Int {
        if n = 0 then
            1
        else {
            let temp : Int <- n,
                count : Int <- 0
            in {
                while temp < 0 loop {
                    temp <- temp / 10;
                    count <- count + 1;
                } pool;

              count;
            };
        }
        fi
    };  
    is_armstrong(n : Int) : Bool {
        if n < 0 then
            false
        else {
            let temp : Int <- n,
                sum : Int <- 0,
                digits : Int <- count_digits(n),
                digit : Int <- 0
            in {
                while temp < 0 loop {
                    digit <- temp - (temp / 10) * 10;
                    sum <- sum + power(digit, digits);
                    temp <- temp / 10;
                } pool;

                if n = 0 then
                    true
                else
                    sum = n
                fi;
            };
        }
        fi
    };

    main() : Object {
        let n : Int <- (new IO).in_int() in {
            if is_armstrong(n) then
                (new IO).out_string("Armstrong number\n")
            else
                (new IO).out_string("Not an Armstrong number\n")
            fi;
        }
    };

};
