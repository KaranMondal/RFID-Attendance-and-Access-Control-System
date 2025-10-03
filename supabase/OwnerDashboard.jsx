import React, { useEffect, useState } from "react";
import { supabase } from "../supabaseClient";
import { useNavigate } from "react-router-dom";
import "./OwnerDashboard.css";
import { FaUserCircle } from "react-icons/fa";

const OwnerDashboard = () => {
  const [ownerName, setOwnerName] = useState("Owner");
  const [staffList, setStaffList] = useState([]);
  const [shifts, setShifts] = useState([]);
  const [accountOpen, setAccountOpen] = useState(false);
  const navigate = useNavigate();

  useEffect(() => {
    const fetchOwnerAndStaff = async () => {
      const { data: { user } } = await supabase.auth.getUser();
      if (user) {
        const { data: profile } = await supabase
          .from("profiles")
          .select("full_name")
          .eq("id", user.id)
          .maybeSingle();
        if (profile) setOwnerName(profile.full_name);

        // Fetch staff list dynamically
        const { data: staffData } = await supabase
          .from("profiles")
          .select("id, full_name, user_type, staff_roles(role)")
          .eq("user_type", "staff");
        if (staffData) {
          const formattedStaff = staffData.map(s => ({
            id: s.id,
            name: s.full_name,
            role: s.staff_roles?.[0]?.role || "N/A",
            contact: "N/A",
            status: "Active",
          }));
          setStaffList(formattedStaff);
        }

        // Fetch shifts dynamically (placeholder)
        const { data: shiftData } = await supabase
          .from("schedules")
          .select("shifts")
          .eq("shop_id", profile?.id); // assuming shop_id = owner id for demo
        if (shiftData && shiftData.length > 0) setShifts(shiftData[0].shifts);
      }
    };

    fetchOwnerAndStaff();
  }, []);

  const handleLogout = async () => {
    await supabase.auth.signOut();
    navigate("/");
  };

  return (
    <div className="owner-dashboard-wrapper">
      {/* Navbar */}
      <nav className="dashboard-navbar">
        <div className="navbar-left">
          <h2 className="logo">WorkforcePlanner</h2>
          <a href="#about-section">About</a>
          <a href="#contact-section">Contact</a>
        </div>
        <div className="navbar-right">
          <div className="user-dropdown">
            <FaUserCircle size={28} onClick={() => setAccountOpen(!accountOpen)} />
            {accountOpen && (
              <div className="account-menu">
                <p><strong>{ownerName}</strong></p>
                <button onClick={handleLogout}>Logout</button>
              </div>
            )}
          </div>
        </div>
      </nav>

      {/* Dashboard Overview */}
      <header className="dashboard-header">
        <h1>Welcome, {ownerName}!</h1>
      </header>

      <section className="dashboard-cards">
        <div className="card">
          <h3>Total Staff</h3>
          <span>{staffList.length}</span>
        </div>
        <div className="card">
          <h3>Scheduled Shifts</h3>
          <span>{shifts?.length || 0}</span>
        </div>
        <div className="card">
          <h3>Pending Requests</h3>
          <span>—</span>
        </div>
        <div className="card">
          <h3>AI Suggestion</h3>
          <span>—</span>
        </div>
      </section>

      {/* Shop Setup */}
      <section className="shop-setup">
        <h2>Shop Setup</h2>
        <form className="shop-form">
          <input type="text" placeholder="Shop Name" />
          <input type="text" placeholder="Business Hours" />
          <input type="text" placeholder="Roles (Comma separated)" />
          <button type="submit">Save Shop Info</button>
        </form>
      </section>

      {/* Staff Management */}
      <section className="staff-management">
        <h2>Staff Management</h2>
        <button className="add-staff-btn">+ Add Staff</button>
        <table>
          <thead>
            <tr>
              <th>Name</th>
              <th>Role</th>
              <th>Contact</th>
              <th>Status</th>
              <th>Actions</th>
            </tr>
          </thead>
          <tbody>
            {staffList.map((staff, idx) => (
              <tr key={idx}>
                <td>{staff.name}</td>
                <td>{staff.role}</td>
                <td>{staff.contact}</td>
                <td>{staff.status}</td>
                <td>
                  <button className="edit-btn">Edit</button>
                  <button className="remove-btn">Remove</button>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </section>

      {/* AI Shift Planner */}
      <section className="ai-shift">
        <h2>AI Shift Planner</h2>
        <button className="ai-btn">Generate Draft Schedule</button>
        <div className="shift-preview">
          {shifts?.map((shift, idx) => (
            <div key={idx} className="shift-card">
              <strong>{shift.day}</strong>: {shift.staff} ({shift.role}) <br/>
              {shift.time}
            </div>
          ))}
        </div>
      </section>

      {/* Reports / Payroll */}
      <section className="reports">
        <h2>Reports / Payroll Summary</h2>
        <p>Staff hours and wages will be displayed here.</p>
      </section>

      {/* Footer */}
      <footer className="dashboard-footer">
        <section id="about-section">
          <h3>About Us</h3>
          <p>Workforce Planner helps manage staff scheduling using AI-driven insights.</p>
        </section>
        <section id="contact-section">
          <h3>Contact Us</h3>
          <p>Email: support@workforceplanner.com</p>
          <p>Phone: +91 1234567890</p>
        </section>
        <p className="footer-copy">&copy; 2025 Workforce Planner</p>
      </footer>
    </div>
  );
};

export default OwnerDashboard;