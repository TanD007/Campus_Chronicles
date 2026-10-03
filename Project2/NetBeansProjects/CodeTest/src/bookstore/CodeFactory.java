/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package bookstore;

/**
 *
 * @author tandi
 */
public class CodeFactory {
    public Code createCode(int methods) {
        double loc = methods * 20 * 0.8;
    return new Code(loc);
    }

    public Code createCode(int methods, int linesPerMethod) {
        double loc = methods * linesPerMethod * 0.8;
    return new Code(loc);
    }

    public Code createCode(int methods, int linesPerMethod, int redundantLinePerMethod) {
    double loc = 0.8 * ((double)(methods * linesPerMethod) / redundantLinePerMethod);
    return new Code(loc);
    }
}
