import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { BehaviorSubject, Observable } from 'rxjs';
import { User } from '../models/user';

@Injectable({
  providedIn: 'root'
})
export class AuthServiceService {

  private currentUserSubject: BehaviorSubject<User> | any;
  public currentUser$: Observable<User> | any;

  constructor(private http: HttpClient) {
    this.currentUserSubject = new BehaviorSubject<User>(localStorage.getItem("currentUser") as any);
    this.currentUser$ = this.currentUserSubject.asObservable();
  }


  public get currentUserValue(): User {
    return this.currentUserSubject.value;
  }

  public getLoggedInUser(): any {
    return this.currentUserSubject.value;
  }

  public isLoggedIn(): boolean{    
    return this.currentUserSubject.value ? true : false;
  }

}
