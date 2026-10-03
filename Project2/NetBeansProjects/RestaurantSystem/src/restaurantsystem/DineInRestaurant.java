/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package restaurantsystem;

/**
 *
 * @author tandi
 */
public class DineInRestaurant extends Restaurant {
    private int kitchenCapacity;

    public DineInRestaurant(int kitchenCapacity, String name, double waitTime) {
        super(name, waitTime);
        this.kitchenCapacity = kitchenCapacity;
    }

    public int getKitchenCapacity() {
        return kitchenCapacity;
    }

    public void setKitchenCapacity(int kitchenCapacity) {
        this.kitchenCapacity = kitchenCapacity;
    }
    

    @Override
    public double getPreparationTime(Item item) {
        if(kitchenCapacity<item.getPrice()/4) return Integer.MAX_VALUE;
        else return getWaitTime() +(item.getPrice()/2) ;
    }
    
}
