/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Main.java to edit this template
 */
package deliverysystem;

/**
 *
 * @author tandi
 */
public class DeliverySystem {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        Customer customer1 = new Customer("Rofiq", 50);
        Customer customer2 = new Customer("Asif", 70);

        Cycle vehicle1 = new Cycle(
                20, "Asgfiq", 20
        );

        Cycle vehicle2 = new Cycle(
                2, "Afi", 70
        );

        Truck vehicle3 = new Truck(
                30, "Rafi", 50
        );

        DeliveryManagement deliveryManagement = new DeliveryManagement();

        deliveryManagement.addVehicle(vehicle1);
        deliveryManagement.addVehicle(vehicle2);
        deliveryManagement.addVehicle(vehicle3);

        DeliveryVehicle bestVehicle1 = deliveryManagement.getBestVehicle(customer1);

        System.out.println(bestVehicle1.getRiderName()
                + " is delivering to " + customer1.getName() + ".");

        DeliveryVehicle bestVehicle2 = deliveryManagement.getBestVehicle(customer2);

        System.out.println(bestVehicle2.getRiderName()
                + " is delivering to " + customer2.getName() + ".");
    }

}
