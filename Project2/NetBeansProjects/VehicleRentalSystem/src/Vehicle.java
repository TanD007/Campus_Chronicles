/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */

/**
 *
 * @author tandi
 */
public class Vehicle {
    private double dailyRate;
    private String Model;

    public Vehicle(double dailyRate, String Model) {
        this.dailyRate = dailyRate;
        this.Model = Model;
    }

    public double getDailyRate() {
        return dailyRate;
    }

    public void setDailyRate(double dailyRate) {
        this.dailyRate = dailyRate;
    }

    public String getModel() {
        return Model;
    }

    public void setModel(String Model) {
        this.Model = Model;
    }
    public void printDetails(){
        System.out.println(Model +" "+ dailyRate);
    }

    
}
