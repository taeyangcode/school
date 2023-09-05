// 8.
// Write a function that receives a string containing a 16-bit binary integer. The
// function must return the string’s integer value.

fn binary_to_integer(binary_input: String) -> String {
    let convert = |result: i16, (index, binary): (usize, char)| {
        return result + (binary as i16 - 0b110000) * 2_i16.pow(index as u32);
    };

    return binary_input
        .chars()
        .into_iter()
        .rev()
        .enumerate()
        .fold(0, convert)
        .to_string();
}

// 9.
// Write a function that receives an integer. The function must return a string
// containing the hexadecimal representation of the integer

fn integer_to_string_hexadecimal(integer_input: isize) -> String {
    if integer_input == 0_isize {
        return "0".to_string();
    }

    let mut result: String = String::new();
    let negative_prefix: String = (if integer_input.is_negative() { "-" } else { "" }).to_string();

    let mut input: f32 = integer_input.abs() as f32;
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

    result.insert_str(0, &negative_prefix);
    return result;
}

fn hexidecimal_add(input_one: String, input_two: String) -> String {
    let mut result: String = String::new();

    let hexidecimal_to_decimal = |hexidecimal: char| -> u8 {
        return match hexidecimal as u8 {
            _ if (0x0..=0x9).contains(&(hexidecimal as u8 - 0x30)) => hexidecimal as u8 - 0x30,
            _ if (0xA..=0xF).contains(&(hexidecimal as u8 - 0x37)) => hexidecimal as u8 - 0x37,
            _ => unreachable!(),
        };
    };

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

    while length_one > index {
        if carry == true {
            let decimal: u8 = hexidecimal_to_decimal(input_one.as_bytes()[length_one - index - 1] as char);
            match decimal == 15 {
                true => result.insert_str(0, &"0"),
                false => { result.insert_str(0, integer_to_string_hexadecimal(decimal as isize + 1).as_str()); carry = false; }
            };
        }
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

    if carry == true {
        result.insert_str(0, &"1");
    }

    return result.trim_start_matches('0').to_string();
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
}
