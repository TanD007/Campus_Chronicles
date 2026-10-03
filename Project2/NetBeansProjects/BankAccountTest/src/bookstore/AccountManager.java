/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package bookstore;

/**
 *
 * @author tandi
 */
public class AccountManager {
    public BankAccount createAccount(double initialDeposit) {
        double balance = initialDeposit;
        return new BankAccount(balance);
    }

    public BankAccount createAccount(double initialDeposit, double interestRate, int years) {

        double balance = initialDeposit * Math.pow((1 + interestRate), years);


        return new BankAccount(balance);
    }


    public BankAccount createAccount(int initialDeposit, int monthlyDeposit, int months) {
        double balance = initialDeposit + ((double) monthlyDeposit / months);
        return new BankAccount(balance);
        }
}

