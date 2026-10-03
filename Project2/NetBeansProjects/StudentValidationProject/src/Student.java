/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */

/**
 *
 * @author tandi
 */
public class Student {

    String studentName;
    String enrollmentYear;

    public Student(String studentName, String enrollmentYear) throws InvalidStudentNameException, InvalidEnrollmentException {
        setStudentName(studentName);
        setEnrollmentYear(enrollmentYear);
    }

    public String getStudentName() {
        return studentName;
    }

    public String getEnrollmentYear() {
        return enrollmentYear;
    }

    

    public void setStudentName(String studentName) throws InvalidStudentNameException {
        if (studentName == null) {
            throw new InvalidStudentNameException("null");
        }
        String[] names = studentName.split(" ");
        if (names.length != 3) {
            throw new InvalidStudentNameException(studentName);
        }
        for (String name : names) {
            if (name.isEmpty()) {
                throw new InvalidStudentNameException(studentName);
            }
            for (int i = 0; i < name.length(); i++) {
                char ch = name.charAt(i);
                if (!Character.isLetterOrDigit(ch) && ch != '-') {
                    throw new InvalidStudentNameException(studentName);
                }
            }
        }
        this.studentName = studentName;
    }

    public void setEnrollmentYear(String enrollmentYear) throws InvalidEnrollmentException{
        if (enrollmentYear == null) {
            throw new InvalidEnrollmentException("null");
        }
        String[] part = enrollmentYear.split(" ");
        if (part.length != 2) {
            throw new InvalidEnrollmentException(enrollmentYear);
        }
        if (!part[0].equals("Spring") && !part[0].equals("Fall")) {
                throw new InvalidEnrollmentException(enrollmentYear);
            }
        if (part[1].isEmpty()) {
                throw new InvalidEnrollmentException(enrollmentYear);
            }
            
            for (int i = 0; i < part[1].length(); i++) {
                
                if (!Character.isDigit(part[1].charAt(i)) ) {
                    throw new InvalidEnrollmentException(enrollmentYear);
                }
            }
            this.enrollmentYear = enrollmentYear;
        }
        
  }
    
    


