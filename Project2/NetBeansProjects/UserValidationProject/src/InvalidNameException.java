/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */

/**
 *
 * @author tandi
 */
public class InvalidNameException extends Exception{

    public InvalidNameException() {
        super("Invalid name.");
    }
    public InvalidNameException(String username) {
        super("Invalid name: " +username);
    }
    
}
