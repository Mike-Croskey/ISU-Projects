package coms319;


import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.net.UnknownHostException;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class Client_login extends JFrame {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;


	/**
	 * Create the frame.
	 * @param myFrame 
	 * @throws IOException 
	 * @throws UnknownHostException 
	 */
	public Client_login() throws UnknownHostException, IOException {
		
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(500, 200, 450, 300);

		
		JPanel listPanel = new JPanel();
		
		JPanel tPane = new JPanel();
		tPane.setLayout(new BoxLayout (tPane, BoxLayout.Y_AXIS));
	
		
		JLabel stuff = new JLabel("M-Chat Client Login");
		stuff.setFont(new Font("Arial",Font.PLAIN,40));
		
		JLabel stuff2 = new JLabel("Please Enter a User Name:");
		stuff2.setFont(new Font("Arial",Font.PLAIN,22));
		
		JTextField name = new JTextField();
		
		JPanel sPanel = new JPanel();
		sPanel.setMaximumSize(new Dimension(300,25));		
		sPanel.setBackground(Color.GRAY);
		sPanel.setLayout(new BorderLayout());
		sPanel.add(name);
		
		
		
		JButton btn1 = new JButton("Login");
		btn1.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {

				System.out.println(name.getText());
				setVisible(false);
				
				try {
					
					Client_chat	myFrame = new Client_chat(name.getText());
					myFrame.setTitle("MChat :: ["+name.getText()+"]");
					
					myFrame.setVisible(true);
				} catch (UnknownHostException e1) {
					e1.printStackTrace();
				} catch (IOException e1) {
					e1.printStackTrace();
				}

			}
		});
		
		
		
		tPane.add(stuff);
	    tPane.add(Box.createRigidArea(new Dimension (0, 10)));

		tPane.add(stuff2);
	    tPane.add(Box.createRigidArea(new Dimension (0, 10)));

		tPane.add(sPanel);
	    tPane.add(Box.createRigidArea(new Dimension (0, 20)));

		tPane.add(btn1);
		tPane.add(Box.createRigidArea(new Dimension (0, 20)));
		
		listPanel.add(tPane);
		
		setContentPane(listPanel);
		
	}
	
}
