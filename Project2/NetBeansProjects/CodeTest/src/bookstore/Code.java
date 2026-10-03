/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package bookstore;

/**
 *
 * @author tandi
 */
public class Code {
    private double linesOfCode;

        public Code() {
        linesOfCode = 250;
    }

    public Code(double linesOfCode) {
        setLinesOfCode(linesOfCode);
    }

    public double getLinesOfCode() {
        return linesOfCode;
    }

    public void setLinesOfCode(double linesOfCode) {
        if (linesOfCode <= 200)
            this.linesOfCode = 250;
        else
            this.linesOfCode = linesOfCode;
    }
}
