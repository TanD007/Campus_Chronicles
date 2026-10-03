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
public class CodeTest {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
Scanner sc = new Scanner(System.in);

CodeFactory factory = new CodeFactory();

int m1 = sc.nextInt();
Code c1 = factory.createCode(m1);

int m2 = sc.nextInt();
int lpm = sc.nextInt();
Code c2 = factory.createCode(m2, lpm);

int m3 = sc.nextInt();
int lpm2 = sc.nextInt();
int r = sc.nextInt();
Code c3 = factory.createCode(m3, lpm2, r);

System.out.printf("Lines of Code: %.2f%n", c1.getLinesOfCode());
System.out.printf("Lines of Code: %.2f%n", c2.getLinesOfCode());
System.out.printf("Lines of Code: %.2f%n", c3.getLinesOfCode());



    }
    
}
