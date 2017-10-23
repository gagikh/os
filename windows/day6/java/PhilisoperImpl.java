package philisopers;

/**
 * Created by Sano on 19.10.2017.
 */
public class PhilisoperImpl implements Philisoper, Runnable {
    private Fork forkRight;
    private Fork forkLeft;
    private String name;


    public PhilisoperImpl(String name) {
        this.name = name;
    }

    @Override
    public void eat() throws InterruptedException {

        System.out.println("Philisoper [ " + this.name + "]::: Eat");
        Thread.sleep(1500);
    }

    @Override
    public void think() throws InterruptedException {
        System.out.println("                               Philisoper [" + this.name + "]::: Think");
        Thread.sleep(1000);
    }

    @Override
    public void run() {
        for (int i = 0; i < 5; i++) {
            System.out.println(i);
            try {
                if (forkLeft != null || forkRight != null) {
                    if (!forkRight.state() && !forkLeft.state()) {
                        forkLeft.setState(true);
                        forkRight.setState(true);
                        eat();
                        forkLeft.setState(false);
                        forkRight.setState(false);
                        Thread.sleep(2000);


                    } else {
                        think();
                    }

                } else {
                    think();
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        System.out.println("****");
    }

    public Fork getForkRight() {
        return forkRight;
    }

    public void setForkRight(Fork forkRight) {
        this.forkRight = forkRight;
    }

    public Fork getForkLeft() {
        return forkLeft;
    }

    public void setForkLeft(Fork forkLeft) {
        this.forkLeft = forkLeft;
    }
}
