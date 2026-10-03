/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Main.java to edit this template
 */
package restaurantsystem;

/**
 *
 * @author tandi
 */
public class RestaurantSystem {

    
    public static void main(String[] args) {
        Item item1 = new Item("Pasta", 15.50);
        Item item2 = new Item("Pizza", 20.00);
        
        DineInRestaurant restaurant1 = new DineInRestaurant(10,"Rooster", 10);
        DineInRestaurant restaurant2 = new DineInRestaurant(0, "Fiore", 25);
        TakeoutRestaurant restaurant3 = new TakeoutRestaurant(5, "Alfresco",  50);
        
        OrderManagement orderManagement = new OrderManagement();
        
        orderManagement.addRestaurant(restaurant1);
        orderManagement.addRestaurant(restaurant2);
        orderManagement.addRestaurant(restaurant3);
        
        Restaurant addRestaurant1 = orderManagement.getFastestRestaurant(item1);
        System.out.println(addRestaurant1.getName() + " is assigned to " + item1.getName() + ".");
        
        Restaurant addRestaurant2 = orderManagement.getFastestRestaurant(item2);
        System.out.println(addRestaurant2.getName() + " is assigned to " + item2.getName() + ".");
    }
    
}
