/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package transport;

/**
 *
 * @author tandi
 */
public class Bus extends Transport implements Refundable{

    public Bus(String name, double baseFare) {
        super(name, baseFare);
    }

    @Override
    public void displayAble(){
        System.out.println("Transport type: Bus"); 
        System.out.println("Name: "+ getName());
        System.out.println("Fare: "+ getBaseFare());
    }
    @Override
    public void refund(){
        System.out.println("Refunded successfully");
    }
}
