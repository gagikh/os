import philisopers.ForkImpl;
import philisopers.PhilisoperImpl;

public class Main {

    public static void main(String[] args) throws InterruptedException {
        int philisoperCount;
        try {
             philisoperCount = Integer.parseInt(args[0] == null ? "3" : args[0]);
        }catch (ArrayIndexOutOfBoundsException e){
            System.out.println("Defualt Parametrs");
            philisoperCount=3;
        }

        PhilisoperImpl[] philisopers = new PhilisoperImpl[philisoperCount];
        ForkImpl[] forks = new ForkImpl[philisoperCount];
        Thread[] threads = new Thread[philisoperCount];
        Integer name = 0; //Philisoper number

        for (int i = 0; i < philisoperCount; i++) {
            forks[i] = new ForkImpl();
            philisopers[i] = new PhilisoperImpl(name.toString());
            name++;
            philisopers[i].setForkRight(forks[i]);
        }

        for (int i = 0; i < philisoperCount; i++) {
            philisopers[i].setForkLeft(forks[(i - 1) < 0 ? philisoperCount-1 : (i - 1)]);
        }

        for (int i = 0; i <philisoperCount; i++) {
            threads[i] = new Thread(philisopers[i]);

        }
        for (Thread t : threads) {
            t.start();
        }
        for (Thread t : threads) {
            t.join();
        }
        System.out.println("Еveryone ate");

    }
}
