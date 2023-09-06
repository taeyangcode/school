#![allow(unused)]

// 8.
// Write a function that receives a string containing a 16-bit binary integer. The
// function must return the string’s integer value.
//
// Converts String formatted 16-bit binary integer to String formatted decimal value.
// ! n = binary_input.len()
// ! Time Complexity: O(n)
// ! Space Complexity: O(1)
//
// @param {String} binary_input - binary to be converted
// @returns {String} - decimal conversion

fn binary_to_integer(binary_input: String) -> String {
    // binary to decimal formula
    // f(index, binary) = binary * 2^(index)
    let convert = |result: i16, (index, binary): (usize, char)| {
        return result + (binary as i16 - 0b110000) * 2_i16.pow(index as u32);
    };

    // 1. converts input to Char iterator
    // 2. reverses input to simplify index usage in binary to decimal formula
    // 3. creates enumerator to have access to current index while iterating
    // 4. fold function that holds an accumulator representing the ongoing decimal sum
    // 5. conversion of calculated i16 sum to String
    return binary_input
        .chars()
        .rev()
        .enumerate()
        .fold(0, convert)
        .to_string();
}

// 9.
// Write a function that receives an integer. The function must return a string
// containing the hexadecimal representation of the integer
//
// Coverts integer to a String formatted hexidecimal
// ! n = integer_input.len()
// ! Time Complexity: O(log_16(n))
// ! Space Complexity: O(1)
//
// @param {isize} integer_input - integer to be converted
// @returns {String} - hexidecimal conversion

fn integer_to_string_hexadecimal(integer_input: isize) -> String {
    // early return 0 conversion
    if integer_input == 0_isize {
        return "0".to_string();
    }

    let mut result: String = String::new();
    // store state to allow for proper conversion of negative integers
    let negative_prefix: String = (if integer_input.is_negative() { "-" } else { "" }).to_string();

    // the absolute value of integer_input is taken to preserve proper negative conversion
    let mut input: f32 = integer_input.abs() as f32;
    // algorithm utilized
    // 1. if the floored value of input is less than or equal to the 0 f32 constant, conversion is
    //    complete
    // 2. input mod 16 computes the decimal representation of the current number in hexidecimal,
    //    requiring another conversion to hexidecimal for values 0xA <= number <= 0xF
    // 3. divide input by 16
    while input.floor() > std::f32::EPSILON {
        let remainder: u8 = (input % 16.0_f32) as u8;
        let hexadecimal: String = match remainder {
            (0x0..=0x9) => remainder.to_string(),
            index if (0xA..=0xF).contains(&remainder) => ["A", "B", "C", "D", "E", "F"][index as usize - 0xA].to_string(),
            _ => unreachable!("expected hexadecimal"),
        };
        result.insert_str(0, &hexadecimal);

        input /= 16.0_f32;
    }

    // if the original input was negative, prefix result with minus sign
    result.insert_str(0, &negative_prefix);
    return result;
}

// 10.
// Write a function that adds two hexadecimal strings, each as long as 1,000 digits.
// Return a hexadecimal string that represents the sum of the inputs.
//
// Adds two String formatted hexadecimals
// ! FINAL ANSWER IS 0 TRIMMED FROM START
// ! n = input_one.len(), m = input_two.len()
// ! Time Complexity: O(max(n, m))
// ! Space Complexity: O(1)
//
// @param {String} input_one - hexadecimal addend
// @param {String} input_two - hexadecimal addend
// @returns {String} - hexadecimal sum

fn hexidecimal_add(input_one: String, input_two: String) -> String {
    let mut result: String = String::new();

    // helper function that converts char representation of a single digit hexidecimal number into
    // its decimal representation (0-15)
    let hexidecimal_to_decimal = |hexidecimal: char| -> u8 {
        return match hexidecimal as u8 {
            _ if (0x0..=0x9).contains(&(hexidecimal as u8 - 0x30)) => hexidecimal as u8 - 0x30,
            _ if (0xA..=0xF).contains(&(hexidecimal as u8 - 0x37)) => hexidecimal as u8 - 0x37,
            _ => unreachable!(),
        };
    };


    // the hexidecimals are iterated over simultaneously in reverse to start addition from the end
    // 1. the sum of the current index of each hexidecimal is calculated (taking the carry into
    //    account)
    // 2. sum mod 16 is the decimal representation of the hexidecimal result at the current index
    // 3. if the sum >= 16, carry bit is set to true, else false
    // 4. increment index
    let mut index: usize = 0;
    let mut carry: bool = false;
    for (letter_one, letter_two) in input_one.chars().into_iter().rev().zip(input_two.chars().into_iter().rev()) {
        let sum: u8 = hexidecimal_to_decimal(letter_one) + hexidecimal_to_decimal(letter_two) + carry as u8;
        let remainder: u8 = sum % 16;
        result.insert_str(0, integer_to_string_hexadecimal(remainder as isize).as_str());
        carry = (sum / 16) != 0;
        index += 1;
    }

    let (length_one, length_two): (usize, usize) = (input_one.len(), input_two.len());

    // as the lengths of input_one and input_two can differ, the following loops account for the
    // remaining digits that need to be inserted into the final result
    // each of the loops also account for an "edge-case" where if the hexidecimal digit at the
    // current index is 'F' and there is a carry from the previous iteration
    while length_one > index {
        if carry == true {
            // convert the hexidecimal digit at current index to decimal
            let decimal: u8 = hexidecimal_to_decimal(input_one.as_bytes()[length_one - index - 1] as char);
            // if the hexidecimal was 0xF (15)
            match decimal == 15 {
                // 0xF -> 0x0 and the carry persists
                true => result.insert_str(0, &"0"),
                // increment hexidecimal and flip carry bit
                false => { result.insert_str(0, integer_to_string_hexadecimal(decimal as isize + 1).as_str()); carry = false; }
            };
        }
        // if there is no carry bit, then the remaining digits can simply be copied
        else {
            result.insert_str(0, &input_one[0..length_one - index]);
            break;
        }
        index += 1;
    }

    while length_two > index {
        if carry == true {
            let decimal: u8 = hexidecimal_to_decimal(input_two.as_bytes()[length_two - index - 1] as char);
            match decimal == 15 {
                true => result.insert_str(0, &"0"),
                false => { result.insert_str(0, integer_to_string_hexadecimal(decimal as isize + 1).as_str()); carry = false; }
            };
        }
        else {
            result.insert_str(0, &input_two[0..length_two - index]);
            break;
        }
        index += 1;
    }

    // if the final addition resulted in a positive carry bit, then prefix the result with a 0x1
    if carry == true {
        result.insert_str(0, &"1");
    }

    // trim any starting 0's from the result
    return result.trim_start_matches('0').to_string();
}

// 11.
// Devise a way of subtracting unsigned binary integers. Test your technique by
// subtracting binary 00000101 from binary 10001000, producing 10000011. Test
// your technique with at least two other sets of integers, in which a smaller value is
// always subtracted from a larger one.
//
// Subtracts two String formatted binary numbers where minuend ≥ subtrahend
// ! FINAL ANSWER IS NOT 0 TRIMMED FROM START
// ! n = binary_one.len()
// ! Time Complexity: O(n)
// ! Space Complexity: O(n), binary_two is copied and prefixed with zeros to match binary_one
// length in order to maintain argument immutability (in-place would be O(1))
//
// @param {String} binary_one - binary minuend
// @param {String} binary_two - binary subtrahend
// @returns {String} - binary difference

fn subtract_binary(binary_one: String, binary_two: String) -> String {
    let mut result: String = String::new();

    // according to the function guidelines, binary_one.len() >= binary_two.len()
    // to allow for easier addition in the case that the lengths are unequal, binary_two is copied
    // and prefixed with '0's until the lengths are equal
    let binary_two: String = format!("{}{}", &"0".repeat(binary_one.len() - binary_two.len()), binary_two.clone());

    // if a carry is required, utilize the following truth table
    //   | 0 | 1 |
    // 0 | 1 | 0 |
    // 1 | 0 | 1 |
    // this truth table can be represented by the compound proposition
    // not (digit_one xor digit_two)
    // or the biconditional
    // digit_one if and only if digit_two
    // the nuance holds in the subtraction of a positive and negative bit, where no carry occurs
    // therefore carry bit can be set to false
    let handle_carry = |digit_one: bool, digit_two: bool, result: &mut String, carry: &mut bool| {
        match (digit_one, digit_two) {
            (true, true) | (false, false) => result.insert_str(0, &"1"),
            (true, false) => { result.insert_str(0, &"0"); *carry = false; },
            (false, true) => result.insert_str(0, &"0"),
        };
    };

    // the formula for handling non carry subtraction is simpler, requiring only an xor operation
    // with a slight nuance for a negative minuend and positive subtrahend (carry)
    // digit_one xor digit_two, if digit_one == 0 && digit_two == 1 then carry bit is true
    let handle_non_carry = |digit_one: bool, digit_two: bool, result: &mut String, carry: &mut bool| {
        match (digit_one, digit_two) {
            (false, true) => { result.insert_str(0, &"1"); *carry = true; },
            _ => result.insert_str(0, if digit_one ^ digit_two { &"1" } else { &"0" }),
        };
    };

    // iterate through each binary input and compute the output using the helper functions above
    let mut carry: bool = false;
    for index in (0..binary_one.len()).rev() {
        let binary_one_digit: bool = (binary_one.as_bytes()[index] - 48) != 0;
        let binary_two_digit: bool = (binary_two.as_bytes()[index] - 48) != 0;

        match carry {
            true => handle_carry(binary_one_digit, binary_two_digit, &mut result, &mut carry),
            false => handle_non_carry(binary_one_digit, binary_two_digit, &mut result, &mut carry),
        };
    }

    return result;
}

#[cfg(test)]
mod test {
    use crate::*;

    #[test]
    fn question_eight() {
        // valid binary conversion tests
        assert!(binary_to_integer("00000000".to_string()) == "0".to_string());
        assert!(binary_to_integer("00000001".to_string()) == "1".to_string());
        assert!(binary_to_integer("00001001".to_string()) == "9".to_string());
        assert!(binary_to_integer("00100011".to_string()) == "35".to_string());

        // invalid input test
        assert!(std::panic::catch_unwind(|| binary_to_integer("non binary input!".to_string())).is_err());
    }

    #[test]
    fn question_nine() {
        // positive hexadecimal conversion tests
        assert!(integer_to_string_hexadecimal(422) == "1A6".to_string());
        assert!(integer_to_string_hexadecimal(123) == "7B".to_string());
        assert!(integer_to_string_hexadecimal(0) == "0".to_string());
        assert!(integer_to_string_hexadecimal(1) == "1".to_string());

        // negative hexadecimal conversion tests
        assert!(integer_to_string_hexadecimal(-1) == "-1".to_string());
        assert!(integer_to_string_hexadecimal(-422) == "-1A6".to_string());
    }

    #[test]
    fn question_ten() {
        // same length additions
        assert!(hexidecimal_add("6A".to_string(), "4B".to_string()) == "B5".to_string());
        assert!(hexidecimal_add("28".to_string(), "58".to_string()) == "80".to_string());
        assert!(hexidecimal_add("28".to_string(), "45".to_string()) == "6D".to_string());
        assert!(hexidecimal_add("36".to_string(), "42".to_string()) == "78".to_string());
        assert!(hexidecimal_add("0A9CC9A89C".to_string(), "61E04D5E9F".to_string()) == "6C7D17073B".to_string());

        // n length addends, n + 1 length sum
        assert!(hexidecimal_add("F".to_string(), "1".to_string()) == "10".to_string());
        assert!(hexidecimal_add("FF".to_string(), "1".to_string()) == "100".to_string());

        // n length addend, m length addend, sum length >= max(n, m)
        assert!(hexidecimal_add("111".to_string(), "FFFFFF".to_string()) == "1000110".to_string());
        assert!(hexidecimal_add("FF25111".to_string(), "1BA95".to_string()) == "FF40BA6".to_string());
        assert!(hexidecimal_add("00000A9CC9A89CC".to_string(), "61E04D5E9F".to_string()) == "10BACE7E86B".to_string());
        assert!(hexidecimal_add("A9CC9A89CC".to_string(), "0000061E04D5E9F".to_string()) == "10BACE7E86B".to_string());
    }

    #[test]
    fn question_eleven() {
        // same length minuend and subtrahend binary subtraction
        assert_eq!(subtract_binary("10001000".to_string(), "00000101".to_string()), "10000011".to_string());
        assert_eq!(subtract_binary("10000".to_string(), "01111".to_string()), "00001".to_string());
        assert_eq!(subtract_binary("110".to_string(), "101".to_string()), "001".to_string());

        // differing length minuend and subtrahend binary subtraction
        assert_eq!(subtract_binary("1011".to_string(), "10".to_string()), "1001".to_string());
        assert_eq!(subtract_binary("11000".to_string(), "111".to_string()), "10001".to_string());
        assert_eq!(subtract_binary("000010001000".to_string(), "00000101".to_string()), "000010000011".to_string());
        assert_eq!(subtract_binary("000010000".to_string(), "01111".to_string()), "000000001".to_string());
    }
}
