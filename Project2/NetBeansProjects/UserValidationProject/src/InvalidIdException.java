/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */

/**
 *
 * @author tandi
 */
public class InvalidIdException extends Exception{

    public InvalidIdException() {
        super("Invalid id.");
    }
    public InvalidIdException(int id) {
        super("Invalid id: "+ id);
    }
    
}
