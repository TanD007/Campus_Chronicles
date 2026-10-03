/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */

/**
 *
 * @author tandi
 */
public class InvalidStudentNameException extends Exception{

    public InvalidStudentNameException() {
        super("Invalid Student Name.");
    }
    public InvalidStudentNameException(String studentName) {
        super("Invalid Student Name: "+ studentName);
    }
}
