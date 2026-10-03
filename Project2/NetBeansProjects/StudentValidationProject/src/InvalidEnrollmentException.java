/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */

/**
 *
 * @author tandi
 */
public class InvalidEnrollmentException extends Exception {

    
    public InvalidEnrollmentException() {
        super("Invalid Enrollment Exception");
    }
    public InvalidEnrollmentException(String enrollmentYear) {
        super("Invalid Enrollment Exception: "+ enrollmentYear);
    }
    
 }
