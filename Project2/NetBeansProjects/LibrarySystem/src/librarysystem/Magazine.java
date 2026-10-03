/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package librarysystem;

/**
 *
 * @author tandi
 */
public class Magazine extends Media{
    private double optimalLevel;

    public Magazine(double optimalLevel, String title, double baseScore) {
        super(title, baseScore);
        this.optimalLevel = optimalLevel;
    }

    public double getOptimalLevel() {
        return optimalLevel;
    }

    public void setOptimalLevel(double optimalLevel) {
        this.optimalLevel = optimalLevel;
    }

    @Override
    public double getSuitability(Member member) {
        return getBaseScore()- (member.getReadingLevel()-getOptimalLevel());
    }
    
}
