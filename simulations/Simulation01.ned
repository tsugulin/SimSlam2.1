network Simulation01
{
    submodules: 
        enter: Create { 
            parameters: 
            	@display("p=50,100;i=block/source");
        } 
        queue1: BalkingQueue { 
            parameters: 
            	@display("p=200,100;i=block/boundedqueue;q=queue");
        } 
        leave1: Terminate { 
            parameters:
            	@display("p=200,200;i=block/sink");
        } 
        queue2: BlockingQueue { 
            parameters: 
            	@display("p=350,100;i=block/boundedqueue;q=queue");
        } 
        leave2: Terminate { 
            parameters:
            	@display("p=500,100;i=block/sink");
        } 
    connections:
        enter.out --> queue1.in; 
        queue1.out_outer --> leave1.in; 
        queue1.out_inner --> queue2.in; 
        queue2.out --> leave2.in; 
}
