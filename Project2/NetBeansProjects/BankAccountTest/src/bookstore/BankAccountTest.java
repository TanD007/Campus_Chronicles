/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Main.java to edit this template
 */
package bookstore;

import java.util.Scanner;

/**
 *
 * @author tandi
 */
public class BankAccountTest {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
    
        Scanner sc = new Scanner(System.in);
        AccountManager manager = new AccountManager();

        System.out.print("Input Initial Deposit: ");
        double initialDeposit1 = sc.nextDouble();
        BankAccount account1 = manager.createAccount(initialDeposit1);

        System.out.print("Input Initial Deposit, Interest rate, year values: ");
        double initialDeposit2 = sc.nextDouble();
        double interestRate = sc.nextDouble();
        int years = sc.nextInt();
        BankAccount account2 = manager.createAccount(initialDeposit2, interestRate, years);

        System.out.print("Input initial deposit, monthly contributions, months values: ");
        int initialDeposit3 = sc.nextInt();
        int monthlyDeposit = sc.nextInt();
        int months = sc.nextInt();
        BankAccount account3 = manager.createAccount(initialDeposit3, monthlyDeposit, months);

        System.out.printf("Balance: %.2f%n", account1.getBalance());
        System.out.printf("Balance: %.2f%n", account2.getBalance());
        System.out.printf("Balance: %.2f%n", account3.getBalance());


        }
}
    

