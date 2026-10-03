/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */

/**
 *
 * @author tandi
 */
public class Fleet {
    private Vehicle[] obj;
    private int count;

    public Fleet(Vehicle[] obj, int count) {
        this.obj = new Vehicle[100];
        this.count = 0;
    }
    public int registerVehicle(Vehicle item){
        obj[count]= item;
        count++;
        return count;
    }
    public void rmvExpensiveRental(double limit){
        for(int i=0;i<count;i++){
            if(obj[i].getDailyRate()>limit){
            for(int j=i;j<count-1;j++){
                obj[j]=obj[j+1];                
            }
            obj[count-1]=null;
            count-- ;
            i--;
            }
        }
        System.out.println(count);
    }
    public void addAt(int i,Vehicle item){
        if(i<0) return;
        for(int j=count;j>i;j--){
            obj[j]=obj[j-1];
        }
        obj[i] =item;
        count++;
    }
    public void listAllVehicles(){
        for(int i=0;i<count;i++){
            obj[i].printDetails();
        }
    }
}
