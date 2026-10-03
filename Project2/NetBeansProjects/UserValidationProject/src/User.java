/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */

/**
 *
 * @author tandi
 */
public class User {
    private String username;
    private int id;

    public User(String username, int id) throws InvalidNameException,InvalidIdException{
        this.username =username;
        this.id = id;
    }

    public String getUsername() {
        return username;
    }

    public int getId() {
        return id;
    }

    public void setUsername(String username) throws InvalidNameException{
        if (username ==null || username.length()==0) {
            throw new InvalidNameException(username==null? "null":username);
        }
        String[] name = username.split(" ");
        if (name.length!=2 || 
                !Character.isUpperCase(name[0].charAt(0)) || !Character.isUpperCase(name[1].charAt(0))) {
            throw new InvalidNameException(username);
        }
        this.username = username;
    }

    public void setId(int id) throws InvalidIdException{
        String idx = String.valueOf(id);
        if (idx.length() !=6 || idx.charAt(2) !=0 || idx.charAt(3) !=5) {
            throw new InvalidIdException();
        }
        this.id = id;
    }
    
}
