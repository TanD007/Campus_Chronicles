/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package company;

/**
 *
 * @author tandi
 */
public class Building {
    private String name;
    private String location;
    private int numofFloors;
    private int roomsperFloor;
    
    private static String specialOffer = "No current offer";
    private static int numofBuildings = 0;
    private static final String MAINTANANCE_POLICY = "Maintanance required every 6 months";
    
    public Building(String name,String location,int numofFloors,int roomsperFloor){
        this.name = name;
        this.location = location;
        this.numofFloors= numofFloors;
        this.roomsperFloor =roomsperFloor ;
        numofBuildings++;
    }
    public int calculateCapacity(){
        return numofFloors*roomsperFloor;
    }
    public void displayInfo(){
        System.out.println("Name: "+ name);
        System.out.println("Location: "+ location);
        System.out.println("Number of floors: "+ numofFloors );
        System.out.println("Rooms per floor: "+ roomsperFloor);
        System.out.println("Capacity: "+calculateCapacity());
        System.out.println("Offer: "+specialOffer);
    }
    
    public static int getTotalBuildings(){
        return numofBuildings;
    }
    public static void setspecialOffer(String offer){
        specialOffer = offer;
    }
    public static void showMaintanance(){
        System.out.println(MAINTANANCE_POLICY);;
    }
    
}
