use std::vec::Vec;
use std::io::{stdin, stdout, Write};

use cli_table::*;

fn get_gametes() -> Vec<String> {
    println!("How many alleles does each gene have?");
    let mut alleles: String = String::new();
    stdin().read_line(&mut alleles).expect("Allele count not provided.");
    let alleles: i32 = alleles.trim().parse::<i32>().expect("Allele count not integer.");

    let mut gametes: Vec<String> = Vec::new();
    for _ in 0..2_i32.pow(alleles as u32) {
        println!("Enter an allele.");
        stdout().flush().unwrap();
        let mut current_allele: String = String::new();
        stdin().read_line(&mut current_allele).expect("Allele not provided.");
        gametes.push(current_allele.trim().to_string());
    }
    return gametes;
}

fn find_genotypes(gametes: Vec<String>) -> Vec<Vec<String>> {
    let mut genotypes: Vec<Vec<String>> = vec![ gametes.clone() ];
    genotypes[0].insert(0, String::new());

    for gene in &gametes {
        let mut vector: Vec<String> = vec![ gene.clone() ];
        vector.resize(gametes.len() + 1, String::new());
        genotypes.push(vector);
    }

    for y in 1..genotypes.len() {
        for x in 1..genotypes[0].len() {
            let length = &genotypes[0][1].len() / 2;
            genotypes[y][x] = String::new();

            for index in 0..length {
                let allele_one = &genotypes[0][x].chars().nth(index).unwrap().clone();
                let allele_two = &genotypes[y][0].chars().nth(index).unwrap().clone();
                if allele_one.is_uppercase() {
                    genotypes[y][x].push(allele_one.clone());
                }
                if allele_two.is_uppercase() {
                    genotypes[y][x].push(allele_two.clone());
                }
            }
            for index in 0..length {
                let allele_one = &genotypes[0][x].chars().nth(index).unwrap().clone();
                let allele_two = &genotypes[y][0].chars().nth(index).unwrap().clone();
                if allele_one.is_lowercase() {
                    genotypes[y][x].push(allele_one.clone());
                }
                if allele_two.is_lowercase() {
                    genotypes[y][x].push(allele_two.clone());
                }
            }
            for index in length..genotypes[0][1].len() {
                let allele_one = &genotypes[0][x].chars().nth(index).unwrap().clone();
                let allele_two = &genotypes[y][0].chars().nth(index).unwrap().clone();
                if allele_one.is_uppercase() {
                    genotypes[y][x].push(allele_one.clone());
                }
                if allele_two.is_uppercase() {
                    genotypes[y][x].push(allele_two.clone());
                }
            }
            for index in length..genotypes[0][1].len() {
                let allele_one = &genotypes[0][x].chars().nth(index).unwrap().clone();
                let allele_two = &genotypes[y][0].chars().nth(index).unwrap().clone();
                if allele_one.is_lowercase() {
                    genotypes[y][x].push(allele_one.clone());
                }
                if allele_two.is_lowercase() {
                    genotypes[y][x].push(allele_two.clone());
                }
            }
        }
    }

    return genotypes;
}

fn main() {
    let gametes: Vec<String> = get_gametes();
    let genotypes: Vec<Vec<String>> = find_genotypes(gametes);
    print_stdout(genotypes.table()).unwrap();
}
