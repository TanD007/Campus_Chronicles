/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package transport;

/**
 *
 * @author tandi
 */
public class Taxi  extends Transport{

    public Taxi(String name, double baseFare) {
        super(name, baseFare);
    }
    @Override
    public void displayAble(){
        System.out.println("Transport type: Taxi"); 
        System.out.println("Name: "+ getName());
        System.out.println("Fare: "+ getBaseFare());
    }
}
