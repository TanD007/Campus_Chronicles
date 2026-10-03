/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package bookstore;

/**
 *
 * @author tandi
 */
public class BankAccount {
    private double balance;

    public BankAccount() {
        this.balance = 200;
    }

    public BankAccount(double balance) {
        setBalance(balance);
    }

    public double getBalance() {
        return balance;
    }

    public void setBalance(double balance) {
        if (balance < 200) {
            this.balance = 200;
        } else {
            this.balance = balance;
        }
    }
}
