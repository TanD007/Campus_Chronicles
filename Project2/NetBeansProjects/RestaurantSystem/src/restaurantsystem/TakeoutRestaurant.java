/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package restaurantsystem;

/**
 *
 * @author tandi
 */
public class TakeoutRestaurant extends Restaurant {
    private int staffCount;

    public TakeoutRestaurant(int staffCount, String name, double waitTime) {
        super(name, waitTime);
        this.staffCount = staffCount;
    }

    public int getStaffCount() {
        return staffCount;
    }

    public void setStaffCount(int staffCount) {
        this.staffCount = staffCount;
    }
    

    @Override
    public double getPreparationTime(Item item) {
        if(staffCount<1) return Integer.MAX_VALUE;
        else return (getWaitTime()/staffCount) +(item.getPrice()) ;
    }
    
}
