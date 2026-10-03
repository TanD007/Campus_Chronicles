/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Main.java to edit this template
 */
package company;

/**
 *
 * @author tandi
 */
public class Company {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        Building building1 = new Building("Uchiha","leaf",6,7);
        Building building2 = new Building("Uzumaki","leaf",7,8);
        Building building3 = new Building("Seven sword","mist",5,7);
        
//        building1.displayInfo();
//        building2.displayInfo();
//        building3.displayInfo();
        
        
        Building.setspecialOffer("Free training for all clans");
        building1.displayInfo();
        building2.displayInfo();
        building3.displayInfo();
        System.out.println("Total Buildings: "+ Building.getTotalBuildings());
        
        Building.showMaintanance();
    }
    
}
