/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package librarysystem;

import java.util.ArrayList;

/**
 *
 * @author tandi
 */
public class Library {
    private  ArrayList<Media> availableMedia;

    public Library() {
        availableMedia = new ArrayList<>();
    }
    public void addMedia(Media media){
        availableMedia.add(media);
        System.out.println(media.getTitle() +" was added");
    }
    public Media recommendMedia(Member member){
        Media bestMedia = null;
        double highScore = Double.MIN_VALUE;
        for (Media media : availableMedia) {
            if(member.getReadingLevel()>highScore){
                bestMedia = media;
                highScore = member.getReadingLevel();
            }
        }
        availableMedia.remove(bestMedia);
        return bestMedia;
    }
}
