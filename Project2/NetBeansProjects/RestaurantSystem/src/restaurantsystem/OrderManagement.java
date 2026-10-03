/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package restaurantsystem;

import java.util.ArrayList;

/**
 *
 * @author tandi
 */
public class OrderManagement {
    private ArrayList<Restaurant>availableRestaurants;

    public OrderManagement() {
        availableRestaurants = new ArrayList<>() ;
    }
    public void addRestaurant(Restaurant nm){
        availableRestaurants.add(nm);
        System.out.println(nm.getName() +" was addwd ");
    }
    public Restaurant getFastestRestaurant(Item item){
        Restaurant fastestRestaurant = null;
        double shortTime = Double.MAX_VALUE;
        for(Restaurant nm : availableRestaurants){
            if(nm.getPreparationTime(item)<shortTime){
                fastestRestaurant = nm;
                shortTime=nm.getPreparationTime(item);
            }
        }
        availableRestaurants.remove(fastestRestaurant);
        return fastestRestaurant;
    }
    
}
