------------------------------------------------------------------------------
-- user_logic.vhd - entity/architecture pair
------------------------------------------------------------------------------
-- Filename:          user_logic.vhd
-- Version:           1.00.a
-- Description:       User logic.
-- Date:              Thu Nov 09 10:44:57 2017
-- VHDL Standard:     VHDL'93
------------------------------------------------------------------------------
-- Naming Conventions:
--   active low signals:                    "*_n"
--   clock signals:                         "clk", "clk_div#", "clk_#x"
--   reset signals:                         "rst", "rst_n"
--   generics:                              "C_*"
--   user defined types:                    "*_TYPE"
--   state machine next state:              "*_ns"
--   state machine current state:           "*_cs"
--   combinatorial signals:                 "*_com"
--   pipelined or register delay signals:   "*_d#"
--   counter signals:                       "*cnt*"
--   clock enable signals:                  "*_ce"
--   internal version of output port:       "*_i"
--   device pins:                           "*_pin"
--   ports:                                 "- Names begin with Uppercase"
--   processes:                             "*_PROCESS"
--   component instantiations:              "<ENTITY_>I_<#|FUNC>"
------------------------------------------------------------------------------

-- DO NOT EDIT BELOW THIS LINE --------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library proc_common_v3_00_a;
use proc_common_v3_00_a.proc_common_pkg.all;

-- DO NOT EDIT ABOVE THIS LINE --------------------

------------------------------------------------------------------------------
-- Entity section
------------------------------------------------------------------------------
-- Definition of Generics:
--   C_NUM_REG                    -- Number of software accessible registers
--   C_SLV_DWIDTH                 -- Slave interface data bus width
--
-- Definition of Ports:
--   Bus2IP_Clk                   -- Bus to IP clock
--   Bus2IP_Resetn                -- Bus to IP reset
--   Bus2IP_Data                  -- Bus to IP data bus
--   Bus2IP_BE                    -- Bus to IP byte enables
--   Bus2IP_RdCE                  -- Bus to IP read chip enable
--   Bus2IP_WrCE                  -- Bus to IP write chip enable
--   IP2Bus_Data                  -- IP to Bus data bus
--   IP2Bus_RdAck                 -- IP to Bus read transfer acknowledgement
--   IP2Bus_WrAck                 -- IP to Bus write transfer acknowledgement
--   IP2Bus_Error                 -- IP to Bus error response
------------------------------------------------------------------------------

entity user_logic is
  generic
  (
    -- ADD USER GENERICS BELOW THIS LINE ---------------
	TIMER_REG_WIDTH					: integer				:= 32;
	CTRL_ENABLE_DEC_BIT				: integer				:= 0;
	CTRL_ENABLE_INT_BIT				: integer				:= 1;
	CTRL_ENABLE_LOOP_BIT			: integer				:= 2;
	
    -- ADD USER GENERICS ABOVE THIS LINE ---------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol parameters, do not add to or delete
    C_NUM_REG                      	: integer              	:= 2;
    C_SLV_DWIDTH                   	: integer              	:= 32
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );
  port
  (
    -- ADD USER PORTS BELOW THIS LINE ------------------
    interrupt                     : out std_logic;
    -- ADD USER PORTS ABOVE THIS LINE ------------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol ports, do not add to or delete
    Bus2IP_Clk                     : in  std_logic;
    Bus2IP_Resetn                  : in  std_logic;		-- low asserted
    Bus2IP_Data                    : in  std_logic_vector(C_SLV_DWIDTH-1 downto 0);
    Bus2IP_BE                      : in  std_logic_vector(C_SLV_DWIDTH/8-1 downto 0);
    Bus2IP_RdCE                    : in  std_logic_vector(C_NUM_REG-1 downto 0);
    Bus2IP_WrCE                    : in  std_logic_vector(C_NUM_REG-1 downto 0);
    IP2Bus_Data                    : out std_logic_vector(C_SLV_DWIDTH-1 downto 0);
    IP2Bus_RdAck                   : out std_logic;
    IP2Bus_WrAck                   : out std_logic;
    IP2Bus_Error                   : out std_logic
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );

  attribute MAX_FANOUT : string;
  attribute SIGIS : string;

  attribute SIGIS of Bus2IP_Clk    : signal is "CLK";
  attribute SIGIS of Bus2IP_Resetn : signal is "RST";

end entity user_logic;

------------------------------------------------------------------------------
-- Architecture section
------------------------------------------------------------------------------

architecture IMP of user_logic is

  --USER signal declarations added here, as needed for user logic********************************************

  ------------------------------------------
  -- Signals for user logic slave model s/w accessible register example
  ------------------------------------------
  signal delay_reg        	: std_logic_vector(C_SLV_DWIDTH-1 downto 0):= (others => '0');
  signal ctrl_reg         	: std_logic_vector(C_SLV_DWIDTH-1 downto 0):= (others => '0');
  signal timer_reg			: std_logic_vector(TIMER_REG_WIDTH-1 downto 0):= (others => '0');
  signal timer_next			: std_logic_vector(TIMER_REG_WIDTH-1 downto 0):= (others => '0');
  signal slv_reg_write_sel	: std_logic_vector(1 downto 0);
  signal slv_reg_read_sel 	: std_logic_vector(1 downto 0);
  signal slv_ip2bus_data  	: std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_read_ack     	: std_logic;
  signal slv_write_ack    	: std_logic;

begin
  slv_reg_write_sel <= Bus2IP_WrCE(1 downto 0);
  slv_reg_read_sel  <= Bus2IP_RdCE(1 downto 0);
  slv_write_ack     <= Bus2IP_WrCE(0) or Bus2IP_WrCE(1);
  slv_read_ack      <= Bus2IP_RdCE(0) or Bus2IP_RdCE(1);

  -- implement slave model software accessible register(s)
  SLAVE_REG_WRITE_PROC : process( Bus2IP_Clk ) is begin
	-- Rising edge?
    if Bus2IP_Clk'event and Bus2IP_Clk = '1' then
      -- Reset registers (synchronous)?
	  if Bus2IP_Resetn = '0' then
        ctrl_reg <= (others => '0');
        delay_reg <= (others => '0');
		timer_reg <= (others => '0');
	  
	  -- Do not reset
	  else	  
		-- load timer
		timer_reg <= timer_next;
	  
        case slv_reg_write_sel is
          when "10" =>
            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
                delay_reg(byte_index*8+7 downto byte_index*8) <= Bus2IP_Data(byte_index*8+7 downto byte_index*8);
              end if;
            end loop;
          when "01" =>
            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
                ctrl_reg(byte_index*8+7 downto byte_index*8) <= Bus2IP_Data(byte_index*8+7 downto byte_index*8);
              end if;
            end loop;
          when others => null;
        end case;
      end if;
    end if;

  end process SLAVE_REG_WRITE_PROC;
	
	-- Next-State Logic
	timer_next <= timer_reg-1 when ctrl_reg(CTRL_ENABLE_DEC_BIT) = '1' and timer_reg > 0 else
				  delay_reg when ctrl_reg(CTRL_ENABLE_LOOP_BIT) = '1'  and timer_reg = 0 else
				  timer_reg;
  
  
  -- implement slave model software accessible register(s) read mux
  SLAVE_REG_READ_PROC : process( slv_reg_read_sel, delay_reg, ctrl_reg) is
	begin
		case slv_reg_read_sel is
			when "10" => slv_ip2bus_data <= delay_reg;
			when "01" => slv_ip2bus_data <= ctrl_reg;
			when others => slv_ip2bus_data <= (others => '0');
		end case;
	end process SLAVE_REG_READ_PROC;
			
	interrupt <= '1' when ctrl_reg(CTRL_ENABLE_INT_BIT) = '1' and timer_reg = X"0000" else '0';
	
	
  -- BUS CONTROL
  IP2Bus_Data  <= slv_ip2bus_data when slv_read_ack = '1' else
                  (others => '0');
  IP2Bus_WrAck <= slv_write_ack;
  IP2Bus_RdAck <= slv_read_ack;
  IP2Bus_Error <= '0';

end IMP;
